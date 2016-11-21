
nv.addGraph(function() {
  var chart = nv.models.multiBarChart()
                .margin({left: 100})  //Adjust chart margins to give the x-axis some breathing room.
//                .useInteractiveGuideline(true)  //We want nice looking tooltips and a guideline!
//                .transitionDuration(350)  //how fast do you want the lines to transition?
                .showLegend(true)       //Show the legend, allowing users to turn on/off line series.
                .showYAxis(true)        //Show the y-axis
                .showXAxis(true)        //Show the x-axis
  ;

  chart.xAxis     //Chart x-axis settings
      .axisLabel('Date')
      .tickFormat(function(d) {
        return d3.time.format('%Y-%m-%d')(new Date(d))
      });

    
  chart.yAxis     //Chart y-axis settings
      .axisLabel('Voltage (v)')
      .tickFormat(d3.format('.0f'));

  var myData = sinAndCos();   //You need data...

  d3.select('#chart2 svg')    //Select the <svg> element you want to render the chart in.   
      .datum(myData)         //Populate the <svg> element with chart data...
      .call(chart);          //Finally, render the chart!

  //Update the chart when window resizes.
  nv.utils.windowResize(function() { chart.update() });
  return chart;
});
/**************************************
 * Simple test data generator
 */
function sinAndCos() {
    var tf = d3.time.format("%Y-%m-%d");
    var t0 = tf.parse("2016-10-10");
    var t1 = tf.parse("2016-10-11");
    var t2 = tf.parse("2016-10-12");
    var t3 = tf.parse("2016-10-13");
    var t4 = tf.parse("2016-10-14");

    console.log(t0,t1,t2,t3,t4);
    
  return [
      {
          values: [{x:t0,y:10}, {x:t1,y:15},{x:t2,y:8},{x:t3,y:11},{x:t4,y:18}],      //values - represents the array of {x,y} data points
          key: 'magician', //key  - the name of the series.
          color: '#ff7f0e'  //color - optional: choose your own line color.
      },
      {
          values: [{x:t0,y:3}, {x:t1,y:7},{x:t2,y:10},{x:t3,y:14},{x:t4,y:28}],     
          key: 'fighter',
          color: '#2ca02c'
      },
      {
          values: [{x:t0,y:1}, {x:t1,y:2},{x:t2,y:1},{x:t3,y:3},{x:t4,y:2}],     
          key: 'druid',
          color: '#7777ff'
          //          area: true      //area - set to true if you want this line to turn into a filled area chart.
      }
  ];
}

