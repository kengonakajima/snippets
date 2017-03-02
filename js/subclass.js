function Shader() {
    this.sh = 1;
}
Shader.prototype.updateMat = function() {
    this.mat=2;
}

FragShader.prototype = Object.create(Shader.prototype);
FragShader.prototype.constructor = FragShader;
function FragShader() {
    Shader.call(this);
    this.updateMat();
}

fsh = new FragShader();

console.log(fsh);