<template>
  <div>
    <h1>Vue.jsで地図アプリ</h1>
    <vmap ref="cmap"></vmap>
    <vcontroller v-on:move-event="move"></vcontroller>
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
