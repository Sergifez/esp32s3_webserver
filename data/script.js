const updateSensorValue = (id, endpoint) => {
fetch(endpoint)
    .then(response => {
    if (!response.ok) {
        throw new Error(`Failed to fetch ${endpoint}`);
    }
    return response.text();
    })
    .then(data => {
    document.getElementById(id).textContent = data;
    })
    .catch(error => {
    console.error(error);
    document.getElementById(id).textContent = 'Error';
    });
};

const ledStatusEl = document.getElementById('led-status');
const ledBlueBtn = document.getElementById('led-blue');
const ledYellowBtn = document.getElementById('led-yellow');
const ledRedBtn = document.getElementById('led-red');
const ledOffBtn = document.getElementById('led-off');

// ===============================
// ELEMENTOS
// ===============================
const img = document.getElementById('stream');
const canvas = document.getElementById('overlay');
const ctx = canvas.getContext('2d');

// ===============================
// HUD LOCAL (tu simulación)
// ===============================
let hud = {
  speed: 0,
  battery: 100,
  lap: 1,
  distance: 0,
  rivalDist: 0
};

let speed = 0;
let gear = 1;
let hp = 1200;

// Para medir FPS del stream (opcional)
let fps = 0;
let lastFps = 0;

setInterval(() => {
    lastFps = fps;
    fps = 0;
}, 1000);

document.getElementById("stream").addEventListener("load", () => {
    fps++;
});

// ===============================
// LATENCIA
// ===============================
let latency = 0;

async function measureLatency() {
    const t0 = performance.now();
    await fetch("/ping");
    latency = performance.now() - t0;
}

setInterval(measureLatency, 1000);

// ===============================
// SIMULACIÓN LOCAL
// ===============================
function updateValues() {
    speed += 0.12;
    if (speed > 260) speed = 0;

    if (speed < 40) gear = 1;
    else if (speed < 80) gear = 2;
    else if (speed < 120) gear = 3;
    else if (speed < 160) gear = 4;
    else if (speed < 200) gear = 5;
    else gear = 6;

    hp = 3000 + Math.floor(Math.sin(speed * 0.03) * 1500);
}

// ===============================
// CANVAS
// ===============================
function resizeCanvas() {
  const container = document.getElementById("cam-container");
  canvas.width = container.clientWidth;
  canvas.height = container.clientHeight;
}

window.addEventListener("load", resizeCanvas);
window.addEventListener("resize", resizeCanvas);

img.addEventListener("load", () => {
  if (canvas.width === 0) resizeCanvas();
});

// ===============================
// DIBUJO DEL HUD (tu código tal cual)
// ===============================
function drawSpeedCircle() {
    const w = canvas.width;
    const h = canvas.height;

    const cx = w * 0.16;
    const cy = h * 0.51;
    const r = h * 0.09;

    ctx.save();
    ctx.shadowColor = "rgba(0, 200, 255, 1)";
    ctx.shadowBlur = h * 0.05;

    ctx.strokeStyle = "rgba(0, 200, 255, 0.8)";
    ctx.lineWidth = h * 0.005;

    ctx.beginPath();
    ctx.arc(cx, cy, r, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();
}

function drawSpeedArc(speed, maxSpeed = 300) {
    const w = canvas.width;
    const h = canvas.height;

    const cx = w * 0.16;
    const cy = h * 0.51;
    const r = h * 0.09;

    const startAngle = 1.2 * Math.PI;
    const endAngle   = 2.8 * Math.PI;

    const progress = speed / maxSpeed;
    const currentAngle = startAngle + (endAngle - startAngle) * progress;

    ctx.save();
    ctx.shadowColor = "rgba(0, 200, 255, 0.9)";
    ctx.shadowBlur = h * 0.03;

    ctx.strokeStyle = "rgba(0, 200, 255, 0.9)";
    ctx.lineWidth = h * 0.02;
    ctx.lineCap = "round";

    ctx.beginPath();
    ctx.arc(cx, cy, r, startAngle, currentAngle);
    ctx.stroke();
    ctx.restore();
}

function drawSpeedometer(speed, gear, hp) {
    const w = canvas.width;
    const h = canvas.height;

    const x = w * 0.16;
    const y = h * 0.53;

    ctx.save();
    ctx.textAlign = "center";

    ctx.shadowColor = "rgba(0, 200, 255, 0.8)";
    ctx.shadowBlur = h * 0.02;

    ctx.fillStyle = "#ffffff";
    ctx.font = (h * 0.055) + "px Orbitron";
    ctx.fillText(speed.toFixed(0), x, y);

    ctx.font = (h * 0.02) + "px Orbitron";
    ctx.fillText("km/h", x, y + h * 0.03);

    ctx.fillStyle = "#ffcc00";
    ctx.font = (h * 0.05) + "px Orbitron";
    ctx.fillText(gear, x - w * 0.03, y + h * 0.12);

    ctx.fillStyle = "#00ffff";
    ctx.font = (h * 0.03) + "px Orbitron";
    ctx.fillText(hp + " HP", x - w * 0.03, y + h * 0.18);

    ctx.restore();
}

function drawStreamData() {
    ctx.fillStyle = "#0ff";
    ctx.font = (canvas.height * 0.03) + "px Orbitron";
    ctx.fillText("FPS: " + lastFps, canvas.width * 0.85, canvas.height * 0.08);
    ctx.fillText("LAT: " + latency.toFixed(0) + " ms", canvas.width * 0.85, canvas.height * 0.12);
}

function drawHUD() {

  ctx.imageSmoothingEnabled = true;
  ctx.clearRect(0, 0, canvas.width, canvas.height);
  updateValues();  // desactivar simulación
  drawSpeedCircle();
  drawStreamData();
  drawSpeedArc(speed);
  drawSpeedometer(speed, gear, hp);

//  drawSpeedArc(hud.speed);
//  drawSpeedometer(hud.speed, gear, hp);

  requestAnimationFrame(drawHUD);
}
drawHUD();

// ===============================
// TELEMETRÍA
// ===============================
async function updateTelemetry() {
  try {
    const r = await fetch("/telemetry", { cache: "no-store" });
    const data = await r.json();

    hud.speed     = data.speed;
    hud.battery   = data.battery;
    hud.lap       = data.lap;
    hud.distance  = data.distance;
    hud.rivalDist = data.rivalDist;

  } catch (e) {
    console.warn("Error telemetría:", e);
  }
}

setInterval(updateTelemetry, 200);

// ===============================
// LED
// ===============================
async function controlLed(color) {
    try {
        const response = await fetch('/api/v1/led', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ state: color })
        });

        const data = await response.json();
        console.log("LED control response:", data);

    } catch (error) {
        console.error("Error controlling LED:", error);
    }
}

document.querySelectorAll(".led-btn").forEach(btn => {
  btn.addEventListener("click", () => {
    const color = btn.dataset.color;
    console.log("LED:", color);
    controlLed(color);
  });
});


// Update all sensors every 10 seconds
setInterval(() => {
    updateSensorValue('temperature', '/temperature');
    updateSensorValue('humidity', '/humidity');
    updateSensorValue('pressure', '/pressure');
}, 10000);
