<template>
  <div>
    <div id="mymap"></div>
  </div>
</template>

<script>

// Leeflet
import L from 'leaflet';
import "leaflet/dist/leaflet.css";

export default{
  name: "vmap",
  mounted: function(){
    this.map = L.map("mymap");
    this.map.setView(this.latlng, this.scale);
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(this.map);
  },
  data: function(){
    return{
      latlng: [35.159399, 136.987255],
      scale: 16
    }
  },
  methods:{
    move(direction){
      this.offset = 0.005
      switch(direction){
        case "left":
        this.latlng = [this.latlng[0], this.latlng[1] - this.offset]
        this.map.panTo(this.latlng)
        break;
        case "up":
        this.latlng = [this.latlng[0] + this.offset, this.latlng[1]]
        this.map.panTo(this.latlng)
        break;
        case "right":
        this.latlng = [this.latlng[0], this.latlng[1] + this.offset]
        this.map.panTo(this.latlng)
        break;
        case "down":
        this.latlng = [this.latlng[0] - this.offset, this.latlng[1]]
        this.map.panTo(this.latlng)
        break;
      }
    }
  }

}
</script>

<style scoped>
#mymap{
  width: 480px;
  height: 480px;
  margin-left: auto;
  margin-right: auto;
}
</style>
