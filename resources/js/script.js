// Function to get the API base URL for the ESP8266
const getApiBaseUrl = () => {
	const useDns = confirm("Do you want to use your ESP8266 local DNS?");

	if (useDns) {
		return "http://esp8266.local";
	} else {
		const ipAddress = prompt("Enter your IP address:");
		return `http://${ipAddress}`;
	}
};

// Set the API base URL
const API_BASE_URL = getApiBaseUrl();

// Common fetch configuration
const fetchConfig = {
	headers: {
		"Content-Type": "application/json",
		Accept: "application/json",
	},
	mode: "cors", // Explicitly set CORS mode
};

const toggleBtn = document.getElementById("toggleBtn");
const speedBtn = document.getElementById("speedBtn");
const alternateBtn = document.getElementById("alternateBtn");
const kamarBtn = document.getElementById("kamarBtn");
const kamarMandiBtn = document.getElementById("kamarMandiBtn");
const ruangTamuBtn = document.getElementById("ruangTamuBtn");

// Status elements
const ledIndicator = document.getElementById("ledIndicator");
const speedIndicator = document.getElementById("speedIndicator");
const alternateIndicator = document.getElementById("alternateIndicator");
const kamarIndicator = document.getElementById("kamarIndicator");
const kamarMandiIndicator = document.getElementById("kamarMandiIndicator");
const ruangTamuIndicator = document.getElementById("ruangTamuIndicator");

const ledStatus = document.getElementById("ledStatus");
const speedStatus = document.getElementById("speedStatus");
const alternateStatus = document.getElementById("alternateStatus");
const kamarStatus = document.getElementById("kamarStatus");
const kamarMandiStatus = document.getElementById("kamarMandiStatus");
const ruangTamuStatus = document.getElementById("ruangTamuStatus");

async function updateStatus() {
	try {
		const response = await fetch(`${API_BASE_URL}/api/status`, {
			method: "GET",
			...fetchConfig,
		});

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		const data = await response.json();

		// Update indicators and text
		ledIndicator.classList.toggle("active", data.ledEnabled);
		speedIndicator.classList.toggle("active", data.fastBlink);
		alternateIndicator.classList.toggle("active", data.alternateMode);
		kamarIndicator.classList.toggle("active", data.ledKamar);
		kamarMandiIndicator.classList.toggle("active", data.ledKamarMandi);
		ruangTamuIndicator.classList.toggle("active", data.ledRuangTamu);

		ledStatus.textContent = data.ledEnabled ? "On" : "Off";
		speedStatus.textContent = data.fastBlink ? "Fast" : "Normal";
		alternateStatus.textContent = data.alternateMode ? "On" : "Off";
		kamarStatus.textContent = data.ledKamar ? "On" : "Off";
		kamarMandiStatus.textContent = data.ledKamarMandi ? "On" : "Off";
		ruangTamuStatus.textContent = data.ledRuangTamu ? "On" : "Off";

		// Update button states
		toggleBtn.classList.toggle("active", data.ledEnabled);
		speedBtn.classList.toggle("active", data.fastBlink);
		alternateBtn.classList.toggle("active", data.alternateMode);
		kamarBtn.classList.toggle("active", data.ledKamar);
		kamarMandiBtn.classList.toggle("active", data.ledKamarMandi);
		ruangTamuBtn.classList.toggle("active", data.ledRuangTamu);
	} catch (error) {
		console.error("Error fetching status:", error);
	}
}

async function sendRequest(endpoint) {
	try {
		const response = await fetch(`${API_BASE_URL}/api/${endpoint}`, {
			method: "POST",
			...fetchConfig,
		});

		if (!response.ok) {
			throw new Error(`HTTP error! status: ${response.status}`);
		}

		await updateStatus();
	} catch (error) {
		console.error(`Error sending ${endpoint} request:`, error);
	}
}


// Add event listeners
toggleBtn.addEventListener("click", () => sendRequest("toggle"));
speedBtn.addEventListener("click", () => sendRequest("speed"));
alternateBtn.addEventListener("click", () => sendRequest("alternate"));
kamarBtn.addEventListener("click", () => sendRequest("kamar"));
kamarMandiBtn.addEventListener("click", () => sendRequest("kamarmandi"));
ruangTamuBtn.addEventListener("click", () => sendRequest("ruangtamu"));

// Update status every second
setInterval(updateStatus, 1000);

// Initial status update
updateStatus();
