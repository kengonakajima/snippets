<template>
  <div>
    <h1>Vue.jsで地図アプリ</h1>
    <vmap ref="cmap"></vmap>
    <vcontroller v-on:move-event="move" v-on:addmarker-event="addmarker"></vcontroller>
  </div>
</template>

<script>
import vmap from "./vmap"
import vcontroller from "./vcontroller"
export default{
  components:{
    vmap,
    vcontroller
  },
  methods:{
    move(direction){
//      alert(direction);
      this.$refs.cmap.move(direction);
    },
    addmarker() {
      this.$refs.cmap.addmarker();
    }
  }
}
</script>

<style scoped>
div{
  width: 640px;
}
h1{
  text-align: center;
}
</style>
