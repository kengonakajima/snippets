const diff = require('diff');

const text1 = `
行1
行2
行3
`;

const text2 = `
行1
行2a
行3
行4
`;

const differences = diff.diffLines(text1, text2);

differences.forEach((part) => {
  // 追加された行は緑色
  if (part.added) {
    console.log('\x1b[32m', part.value);
  }
  // 削除された行は赤色
  else if (part.removed) {
    console.log('\x1b[31m', part.value);
  }
  // 変更がない行はそのまま
  else {
    console.log(part.value);
  }
});
