numToHumanReadable = function(n) {
    var i = -1;
    var units = [' K', ' M', ' G', ' T', 'P', 'E', 'Z', 'Y'];
    do {
        n = n / 1000;
        i++;
    } while (n > 1000);

    return Math.max(n, 0.1).toFixed(1) + units[i];
};

console.log(numToHumanReadable(100));
console.log(numToHumanReadable(4000));
console.log(numToHumanReadable(999900));
console.log(numToHumanReadable(48484849100));
console.log(numToHumanReadable(48484849100000000000));