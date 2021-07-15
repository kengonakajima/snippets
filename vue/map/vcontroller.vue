<template>
  <div>
    <div id="mycontroller">
      <div id="mypad">
        <div class="pad" v-on:click="move('left')">LEFT</div>
        <div class="pad" v-on:click="move('up')">UP</div>
        <div class="pad" v-on:click="move('right')">RIGHT</div>
        <div class="pad" v-on:click="move('down')">DOWN</div>        
      </div>
    </div>
  </div>
</template>

<script>

//import Vue from "vue";

export default{
  name: "vcontroller",
  methods:{
    move(direction){
      // 親コンポーネントであるmain.vueのmoveメソッドを呼び出す      
      this.$emit("move-event", direction)
    }
  }
  
}
</script>

<style scoped>
#mycontroller{
  width: 480px;
  margin-top: 20px;
  margin-left: auto;
  margin-right: auto;
}
#mypad{
  display: flex;
  justify-content: space-around;
}
.pad{
  display: block;
  width: 30px;
  height: 30px;
  border: 1px solid black;
}
</style>
