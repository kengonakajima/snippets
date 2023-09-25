const puppeteer = require('puppeteer');

(async () => {
	const browser = await puppeteer.launch({
		headless: false,  // ブラウザが動く様子を確認する
		slowMo: 50,
	})
	//chromeを開く
	const page = await browser.newPage()

	//指定ページを開く。
	await page.goto('https://www.google.com/?hl=ja')

	//10秒待つ
	await page.waitForTimeout(10000);

	//ブラウザを閉じる
	await browser.close()
})()
