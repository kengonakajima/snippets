// Promiseを使用してsetTimeoutをラップする関数
function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// async関数を使用して非同期処理を実行
async function run() {
    // 1回目の3秒待つ
    await delay(3000);
    console.log("1回目の3秒経過しました！");

    // 2回目の3秒待つ
    await delay(3000);
    console.log("2回目の3秒経過しました！");
}

run();

