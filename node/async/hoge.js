// Promiseを使用してsetTimeoutをラップする関数
function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// 1回目の3秒待つ
delay(3000)
    .then(() => {
        console.log("1回目の3秒経過しました！");
        // 2回目の3秒待つ
        return delay(3000);
    })
    .then(() => {
        console.log("2回目の3秒経過しました！");
    });

