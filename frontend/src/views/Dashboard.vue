<template>
    <VRow align = "left">
        <VCol class = "col1">
            <v-card color = "surface">
                <v-sheet label = "Height(in)">
                    <v-slider direction = "vertical" thumb-label = "always" class = "slider" color = "green" track-size = "60"></v-slider>
                </v-sheet>
            </v-card>
        </VCol>
        
        <v-col cols="10">
            <!--area graph-->
            <figure class="highcharts-figure">
            <div id="container"></div>
            </figure>
      </v-col>
    </VRow>

    <VRow>
        <!-- gauge graph -->
        <VCol>
            <v-card color = "surface">
                <figure class="highcharts-figure">
                    <div id="container0"></div>
                </figure>
            </v-card>
        </VCol>

        <v-sheet max-width="350px">
          <v-card class="mb-5" style="max-width: 350px" variant="tonal" color="primary" density="compact" rounded="lg"
            title="Water Level" subtitle="Capacity Remaining" align="center">
                <div id="fluid-meter"></div>

            <v-dialog width="500" v-model="isActive">
              <template v-slot:default="{ isActive }">
                <v-card title="Overflow Detected" color="blue" background-color="surface" align="center">
                  <v-card-actions>
                    <v-spacer></v-spacer>
                  </v-card-actions>
                </v-card>
              </template>
            </v-dialog>

          </v-card>
        </v-sheet>
    </VRow>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";
import { useMqttStore } from '@/store/mqttStore'; // Import Mqtt Store
import { useAppStore } from "@/store/appStore"; 
import {storeToRefs} from "pinia";

const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);  
const host = ref("www.yanacreations.com");
const port = ref(9002);
const point = ref(10);
const shift = ref(false);
let isActive = ref(false);
 
// VARIABLES
const router      = useRouter();
const route       = useRoute();  

const waterHeight = computed(() => {
  if (!!payload.value) {
    return '${payload.value.waterHeight.toFixed(2)} inches';
  }
}
);

const reserves = computed(() => {
  if (!!payload.value) {
    return '${payload.value.reserves.toFixed(2)} gallons';
  }
}
);

const percentage = computed(() => {
  if (!!payload.value) {
    return '${payload.value.percentage.toFixed(2)}';
  }
}
);

const resChart = ref(null);
const resGauge = ref(null);
const slider = ref(50);

const createCharts = async () => {
    resChart.value = Highcharts.chart("container", {
    chart: {
      zoomType: "x",
      animation: false
    },
    title: { text: "Water Reserves (10 mins)", align: "left" },

    yAxis: {
      title: {
        text: "Water Level",
        style: { color: "#000000" },
      },
      labels: { format: "{value} gal" },
    },
    xAxis: {
      type: "datetime",
      title: { text: "Time", style: { color: "#000000" } },
    },
    tooltip: { shared: true },
    series: [
      {
        name: "Water",
        type: "area",
        data: [],
        turboThreshold: 0,
        color: Highcharts.getOptions().colors[0],
        // pointWidth: 1000,
      },
    ],
    plotOptions: {
      column: {
        pointPadding: 0,
        borderWidth: 0,
        groupPadding: 0,

        shadow: false
      },
      bar: {
        horizontal: false,
        columnWidth: '100%',
        endingShape: 'rounded',
      },
    },
  });

  resGauge.value = Highcharts.chart("container0", {
    title: { text: 'Water Reserves', align: 'left' },
    // the value axis
    yAxis: {
      min: 0,
      max: 1000,
      tickPixelInterval: 72,
      tickPosition: 'inside',
      tickColor: Highcharts.defaultOptions.chart.backgroundColor || '#FFFFFF',
      tickLength: 20,
      tickWidth: 2,
      minorTickInterval: null,
      labels: { distance: 20, style: { fontSize: '14px' } },
      lineWidth: 0,
      plotBands: [{ from: 0, to: 200, color: '#DF5353', thickness: 20 }, {
        from: 200, to: 600, color: '#DDDF0D', thickness: 20
      }, { from: 600, to: 1000, color: '#55BF3B', thickness: 20 }]
    },
    tooltip: { shared: true, },
    pane: { startAngle: -90, endAngle: 89.9, background: null, center: ['50%', '75%'], size: '110%' },
    series: [{
      type: 'gauge',
      name: 'Water Capacity',
      data: [0],
      tooltip: { valueSuffix: ' Gal' },
      dataLabels: {
        format: '{y} Gal', borderWidth: 0, color: (Highcharts.defaultOptions.title &&
          Highcharts.defaultOptions.title.style && Highcharts.defaultOptions.title.style.color) || '#333333', style: { fontSize: '16px' }
      },
      dial: { radius: '80%', backgroundColor: 'gray', baseWidth: 12, baseLength: '0%', rearLength: '0%' },
      pivot: { backgroundColor: 'gray', radius: 6 }
    }],
  });
}

// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED

    Mqtt.connect(); // Connect to Broker located on the backend
    setTimeout(() => {
    // Subscribe to each topic
    Mqtt.subscribe("620162688");
    Mqtt.subscribe("620162688_pub");
    Mqtt.subscribe("620162688_sub");
  }, 3000);

  createCharts();
});


onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
    Mqtt.unsubcribeAll();
});

watch(payload, (data) => {
  // THIS FUNCTION IS CALLED WHEN THE VALUE OF THE VARIABLE "payload" CHANGES

  if (resChart.value.series[0].points.value > 500) { resChart.value.series[0].points.value--; }
  else { shift.value = true; }

  slider1.value = data.radar

  if (data.waterHeight >= 77) {
    fm.setPer(100);
    isActive.value = true;

    resChart.value.series[0].addPoint({ y: parseFloat(data.reserve.toFixed(2)), x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    resGauge.value.series[0].points[0].update(1000); // Add new data point
  }
  else if (data.waterHeight <= 0) {
    fm.setPer(0);
    resChart.value.series[0].addPoint({ y: 0, x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    resGauge.value.series[0].points[0].update(0); // Add new data point
    isActive.value = false;

  }
  else {
    fm.setPer(data.percentage.toFixed(2));
    resChart.value.series[0].addPoint({ y: parseFloat(data.reserve.toFixed(2)), x: data.timestamp * 1000 }, true, shift.values); // Add new data point
    resGauge.value.series[0].points[0].update(parseFloat(data.reserve.toFixed(2)));
    isActive.value = false;
  }
});

/*
const data = {
datasets: [
{
label: 'Water',
data: [0, 0, 0, 0, 0, 0],
borderColor: '#000000',
backgroundColor: '#FFFFFF',
borderWidth: 2,
borderRadius: 5,
borderSkipped: false,
}]
};

const config = { type: 'area',
    data: data,
    options: {
        responsive: true,
        plugins: {
        title: {
            align: screenLeft,
            display: true,
            text: "Water Reserves (10 mins)"
        },
        zooming:{
            zoomType: "x"
        },
        yAxis:[{
            title: "Water Level",

            labels:{
                format: "{value} gal"
            }
        }],
        xAxis:[{
            type: "datetime",
            title: "Time"
        }]
        }
    },
};

const data2 = {
datasets: [
{
label: 'Water',
data: [0, 0, 0, 0, 0, 0],
borderColor: '#000000',
backgroundColor: '#FFFFFF',
borderWidth: 2,
borderRadius: 5,
borderSkipped: false,
}]
};

const options = {
    type: 'radial-gauge',
    container: document.getElementById('myChart'),
    value: 80,  //get value 
    scale: {
        min: 0,
        max: 1000,
        secondaryLabel:{
            text: "{value} gal"
        }
    },
    needle:{
        enabled: true,
    }
};

AgCharts.createGauge(options);
*/
var fm = new FluidMeter();
fm.init({
  targetContainer: document.getElementById("fluid-meter"),
  fillPercentage: percentage,
  options: {
    fontSize: "70px",
    fontFamily: "Arial",
    fontFillStyle: "white",
    drawShadow: true,
    drawText: true,
    drawPercentageSign: true,
    drawBubbles: true,
    size: 300,
    borderWidth: 25,
    backgroundColor: "#e2e2e2",
    foregroundColor: "#fafafa",
    foregroundFluidLayer: {
      fillStyle: "green",
      angularSpeed: 100,
      maxAmplitude: 12,
      frequency: 30,
      horizontalSpeed: -150
    },
    backgroundFluidLayer: {
      fillStyle: "pink",
      angularSpeed: 100,
      maxAmplitude: 9,
      frequency: 30,
      horizontalSpeed: 150
    }
  }
});

fm.setPer(percentage);

</script>


<style scoped>
/** CSS STYLE HERE */
.slider{
    width: 150px;
    color: olive;
}

.col1{
    width: 100px;
}

.col2{
    width:500px;
}
</style>
  