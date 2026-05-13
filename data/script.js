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

// Control LED
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

// Event Listeners
ledBlueBtn.addEventListener('click', () => controlLed('blue'));
ledYellowBtn.addEventListener('click', () => controlLed('yellow'));
ledRedBtn.addEventListener('click', () => controlLed('red'));
ledOffBtn.addEventListener('click', () => controlLed('off'));

// Update all sensors every 10 seconds
setInterval(() => {
    updateSensorValue('temperature', '/temperature');
    updateSensorValue('humidity', '/humidity');
    updateSensorValue('pressure', '/pressure');
}, 10000);
