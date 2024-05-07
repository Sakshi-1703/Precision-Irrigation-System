// Simulated sensor data
let crops = {
    'Pepper Plant': {
        temperature: Math.floor(Math.random() * 30) + 10,
        humidity: Math.floor(Math.random() * 50) + 40,
        soilMoisture: Math.floor(Math.random() * 80) + 20
    },
    'Bean Crops': {
        temperature: Math.floor(Math.random() * 30) + 10,
        humidity: Math.floor(Math.random() * 50) + 40,
        soilMoisture: Math.floor(Math.random() * 80) + 20
    }
};

// Display sensor readings
const sensorDataContainer = document.getElementById('sensor-data');
Object.keys(crops).forEach(crop => {
    const data = crops[crop];
    const cropDiv = document.createElement('div');
    cropDiv.classList.add('crop-data');
    cropDiv.innerHTML = `<h3>${crop}</h3>
                          <p>Temperature: ${data.temperature} °C</p>
                          <p>Humidity: ${data.humidity} %</p>
                          <p>Soil Moisture: ${data.soilMoisture} %</p>`;
    sensorDataContainer.appendChild(cropDiv);
});

// Simulated water flow
setInterval(() => {
    const flowRate = Math.floor(Math.random() * 10) + 1;
    document.getElementById("flow").innerHTML = flowRate;
}, 5000); // Simulate every 5 seconds

// Display sensor graph
const sensorChartCanvas = document.getElementById('sensorChart').getContext('2d');
const sensorChart = new Chart(sensorChartCanvas, {
    type: 'bar',
    data: {
        labels: Object.keys(crops),
        datasets: [{
            label: 'Temperature (°C)',
            data: Object.values(crops).map(data => data.temperature),
            backgroundColor: 'rgba(255, 99, 132, 0.2)',
            borderColor: 'rgba(255, 99, 132, 1)',
            borderWidth: 1
        },
        {
            label: 'Humidity (%)',
            data: Object.values(crops).map(data => data.humidity),
            backgroundColor: 'rgba(54, 162, 235, 0.2)',
            borderColor: 'rgba(54, 162, 235, 1)',
            borderWidth: 1
        },
        {
            label: 'Soil Moisture (%)',
            data: Object.values(crops).map(data => data.soilMoisture),
            backgroundColor: 'rgba(255, 206, 86, 0.2)',
            borderColor: 'rgba(255, 206, 86, 1)',
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});

// Function to toggle irrigation
function toggleIrrigation() {
    const statusElement = document.getElementById('irrigation-status');
    if (statusElement.textContent === 'Inactive') {
        statusElement.textContent = 'Active';
        alert('Irrigation started'); // Placeholder for actual irrigation control
    } else {
        statusElement.textContent = 'Inactive';
        alert('Irrigation stopped'); // Placeholder for actual irrigation control
    }
}

// Function to add a new crop
function addCrop() {
    const newCropName = prompt('Enter the name of the new crop:');
    if (newCropName) {
        crops[newCropName] = {
            temperature: Math.floor(Math.random() * 30) + 10,
            humidity: Math.floor(Math.random() * 50) + 40,
            soilMoisture: Math.floor(Math.random() * 80) + 20
        };

        const newCropDiv = document.createElement('div');
        newCropDiv.classList.add('crop-data');
        newCropDiv.innerHTML = `<h3>${newCropName}</h3>
                                <p>Temperature: ${crops[newCropName].temperature} °C</p>
                                <p>Humidity: ${crops[newCropName].humidity} %</p>
                                <p>Soil Moisture: ${crops[newCropName].soilMoisture} %</p>`;
        sensorDataContainer.appendChild(newCropDiv);

        updateSensorGraph();
    }
}

// Function to update sensor graph
function updateSensorGraph() {
    sensorChart.data.labels = Object.keys(crops);
    sensorChart.data.datasets[0].data = Object.values(crops).map(data => data.temperature);
    sensorChart.data.datasets[1].data = Object.values(crops).map(data => data.humidity);
    sensorChart.data.datasets[2].data = Object.values(crops).map(data => data.soilMoisture);
    sensorChart.update();
}

// Display water volume graph
const waterVolumeGraphCanvas = document.getElementById('waterVolumeChart').getContext('2d');
const waterVolumeChart = new Chart(waterVolumeGraphCanvas, {
    type: 'bar',
    data: {
        labels: ['Day 1', 'Day 2', 'Day 3', 'Day 4', 'Day 5'],
        datasets: [{
            label: 'Water Volume (liters)',
            data: [50, 60, 55, 70, 65], // Example data, replace with actual data
            backgroundColor: 'rgba(54, 162, 235, 0.2)',
            borderColor: 'rgba(54, 162, 235, 1)',
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            y: {
                beginAtZero: true
            }
        }
    }
});