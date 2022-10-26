const delay = require('delay');
const aperture = require('aperture')();

const options = {
	fps: 30,
	cropArea: {
		x: 100,
		y: 100,
		width: 500,
		height: 500
	}
};

(async () => {
	await aperture.startRecording(options);
	await delay(3000);
	console.log(await aperture.stopRecording());
	//=> '/private/var/folders/3x/jf5977fn79jbglr7rk0tq4d00000gn/T/cdf4f7df426c97880f8c10a1600879f7.mp4'
})();
