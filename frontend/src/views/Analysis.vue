<template>
    <v-container fluid>
        <VRow class = "row, row1">
            <VCol>
                <v-sheet class = "sheet">
                    <div></div>
                    <v-text-field class = "textField, textField1" label = "Start Date" type = "Date" density = "compact" variant = "solo-inverted" flat v-model="start"></v-text-field>
                    <v-text-field class = "textField" label = "End Date" type = "Date" density = "compact" variant = "solo-inverted" flat v-model="end"></v-text-field>
                    <v-spacer></v-spacer>
                    <VBtn @click="updateLineCharts(); updateColumnCharts(); updateScatterCharts(); updateCards(); updateHistogramCharts()" class = "text-caption" text = "Analyse" color = "primary" variant = "tonal"></VBtn>
                </v-sheet>
            </VCol>

            <VCol cols = "3" align = "center">
                <v-card title = "Average" width = "250" variant = "outlined" color = "primary" density = "compact" rounded = "lg" subtitle = "For the selected period">
                    <v-card-item margin-bottom = "n5">
                        <v-chip-group class = "d-flex-flex-row justify-center" color = "primaryContainer" variant = "flat">
                            <v-tooltip text = "Min" location = "start"><v-chip>{{ }}</v-chip></v-tooltip>
                        </v-chip-group>
                    </v-card-item>
                </v-card>
            </VCol>
        </VRow>

        <VRow class = "row">
            <VCol cols = "12">
                <figure class = "highcharts-figure">
                    <div id = "container">
                        <cavnas id = "chart"></cavnas>
                    </div>
                </figure>
            </VCol>

            <VCol cols = "12">
                <figure class = "highcharts-figure">
                    <div id = "container0">
                        <canvas id = "chart0"></canvas>
                    </div>
                </figure>
            </VCol>
        </VRow>
    </v-container>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";
import { useMqttStore } from '@/store/mqttStore'; // Import Mqtt Store
import { useAppStore } from "@/store/appStore"; 
import {storeToRefs} from "pinia";
import Chart from 'chart.js/auto'; 
 
 
// VARIABLES
const router = useRouter();
const route = useRoute();  
const AppStore = useAppStore();
const start = ref("");
const end = ref("");
const temperature = reactive({"min":0,"max":0,"avg":0,"range":0});
const humidity = reactive({"min":0,"max":0,"avg":0,"range":0});
let chart = null;

const Mqtt = useMqttStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);  

const data = {
datasets: [
{
label: 'Reserve',
data: [0, 0, 0, 0, 0, 0],
borderColor: '#1ECBE1',
backgroundColor: '#4BD5E7',
borderWidth: 2,
borderRadius: 5,
borderSkipped: false,
}]
};

const data0 = {
datasets: [
{
label: 'Analysis',
data: [0, 0, 0, 0, 0, 0],
borderColor: '#1ECBE1',
backgroundColor: '#4BD5E7',
borderWidth: 2,
borderRadius: 5,
borderSkipped: false,
}]
};

const config = { type: 'line',
    data: data,
    options: {
        responsive: true,
        plugins: {
        title: {
            align: screenLeft,
            display: true,
            text: 'Water Management Analysis'
        },
        subtitle:{
            text: "The heat index, also known as the 'apparent temperature,' is a measure that combines air temperature and relative humidity to assess how hot it feels to the human body. The relationship between heat index and air temperature is influenced by humidity levels. As humidity increases, the heat index also rises, making the perceived temperature higher than the actual air temperature."
        },
        zooming:{
            zoomType: "x"
        },
        tooltip:{
            pointFormat: "",
            shared: true
        },
        yAxis:[{
            title: "Water Reserve",

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

const config0 = { type: 'scatter',
    data: data,
    series:{name: "Analysis"},
    options: {
        responsive: true,
        plugins: {
        title: {
            align: screenLeft,
            display: true,
            text: 'Height and Water Level Correlation Analysis'
        },
        zooming:{
            zoomType: "x"
        },
        tooltip:{
            pointFormat: "Temperature: {point.x} °C <br/> Heat Index: {point.y} °C",
            shared: true
        },
        yAxis:[{
            title: "Height",

            labels:{
                format: "{value} in"
            }
        }],
        xAxis:[{
            type: "datetime",
            title: "Water Height",

            labels:{
                format: "{value} in"
            }
        }]
        }
    },
};


// FUNCTIONS
onMounted(()=>{
    // THIS FUNCTION IS CALLED AFTER THIS COMPONENT HAS BEEN MOUNTED
    CreateCharts();
    /*const ctx = document.querySelector('#chart'); // Select canvas for rendering chart
    chart = new Chart(ctx, config ); // create chart

    const ctx0 = document.querySelector('#chart0'); // Select canvas for rendering chart
    chart0 = new Chart(ctx0, config0 ); // create chart*/

    /*const ctx1 = document.querySelector('#chart1'); // Select canvas for rendering chart
    chart1 = new Chart(ctx1, config1 ); // create chart

    const ctx2 = document.querySelector('#chart2'); // Select canvas for rendering chart
    chart2 = new Chart(ctx2, config2 ); // create chart

    const ctx3 = document.querySelector('#chart3'); // Select canvas for rendering chart
    chart3 = new Chart(ctx3, config3 ); // create chart*/
});

onBeforeUnmount(()=>{
    // THIS FUNCTION IS CALLED RIGHT BEFORE THIS COMPONENT IS UNMOUNTED
});

const CreateCharts = async () => {
// TEMPERATURE CHART
tempHiChart.value = Highcharts.chart('container', {
    chart: { zoomType: 'x' },
    title: { text: 'Air Temperature and Heat Index Analysis', align: 'left' },
    yAxis: {
        title: { text: 'Air Temperature & Heat Index' , style:{color:'#000000'}},
        labels: { format: '{value} °C' }
    },

    xAxis: {
        type: 'datetime',
        title: { text: 'Time', style:{color:'#000000'} },
    },

    tooltip: { shared:true, },

    series: [
        {
            name: 'Temperature',
            type: 'spline',
            data: [],
            turboThreshold: 0,
            color: Highcharts.getOptions().colors[0]
        },
        {
            name: 'Heat Index',
            type: 'spline',
            data: [],
            turboThreshold: 0,
            color: Highcharts.getOptions().colors[1]
    } ],
});

humidHiChart.value = Highcharts.chart('container1', {
    chart: { zoomType: 'x' },
    title: { text: 'Humidity Analysis (Live)', align: 'left' },
    yAxis: {
        title: { text: 'Air Temperature & Heat Index' , style:{color:'#000000'}},
        labels: { format: '{value} %' }
    },

    xAxis: {
        type: 'datetime',
        title: { text: 'Time', style:{color:'#000000'} },
    },

    tooltip: { shared:true, },

    series: [
        {
            name: 'Temperature',
            type: 'spline',
            data: [],
            turboThreshold: 0,
            color: Highcharts.getOptions().colors[0]
        },
        {
            name: 'Heat Index',
            type: 'spline',
            data: [],
            turboThreshold: 0,
            color: Highcharts.getOptions().colors[1]
    } ],
});
};

const updateLineCharts = async ()=>{
    if(!!start.value && !!end.value){
        // Convert output from Textfield components to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;

        // Fetch data from backend
        const data = await AppStore.getAllInRange(startDate,endDate);

        // Create arrays for each plot
        let temperature = [];
        let heatindex = [];

        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            temperature.push({"x": row.timestamp * 1000, "y": parseFloat(row.temperature.toFixed(2)) });
            heatindex.push({ "x": row.timestamp * 1000,"y": parseFloat(row.heatindex.toFixed(2)) });
        });

        // Add data to Temperature and Heat Index chart
        tempChart.value.series[0].setData(temperature);
        tempChart.value.series[1].setData(heatindex);
    }
}

const updateScatterCharts = async ()=>{
    if(!!start.value && !!end.value){
        // Convert output from Textfield components to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;

        // Fetch data from backend
        const data = await AppStore.getAllInRange(startDate,endDate);

        // Create arrays for each plot
        let temperature = [];
        let heatindex = [];

        // Iterate through data variable and transform object to format recognized by highcharts
        data.forEach(row => {
            temperature.push({"x": row.timestamp * 1000, "y": parseFloat(row.temperature.toFixed(2)) });
            heatindex.push({ "x": row.timestamp * 1000,"y": parseFloat(row.heatindex.toFixed(2)) });
        });

        // Add data to Temperature and Heat Index chart
        tempChart.value.series[0].setData(temperature);
        tempChart.value.series[1].setData(heatindex);
    }
}

const updateCards = async () => {
    // Retrieve Min, Max, Avg, Spread/Range
    if(!!start.value && !!end.value){
        // 1. Convert start and end dates collected fron TextFields to 10 digit timestamps
        let startDate = new Date(start.value).getTime() / 1000;
        let endDate = new Date(end.value).getTime() / 1000;

        // 2. Fetch data from backend by calling the API functions
        const temp = await AppStore.getTemperatureMMAR(startDate,endDate);
        const humid = await AppStore.getHumidityMMAR(startDate,endDate);
        console.log(humid)
        temperature.max = temp[0].max.toFixed(1);

        //3. complete for min, avg and range

        //4. complete max, min, avg and range for the humidity variable

    }
}

const updateHistogramCharts = async () =>{
    // Retrieve Min, Max, Avg, Spread/Range for Column graph
    if(!!start.value && !!end.value){
        // 1. Convert start and end dates collected fron TextFields to 10 digit timestamps
        // Subsequently, create startDate and endDate variables and then save the respective timestamps in these variables
        // 2. Fetch data(temp, humid and hi) from backend by calling the getFreqDistro API functions for each
        const temp = await AppStore.getFreqDistro("temperature",startDate,endDate);
        const humid = await AppStore.getFreqDistro("humidity",startDate,endDate);
        const hi = await AppStore.getFreqDistro("heatindex",startDate,endDate);
        // 3. create an empty array for each variable (temperature, humidity and heatindex)
        // see example below
        let temperature = [];
        // 4. Iterate through the temp variable, which contains temperature data fetched from the backend
        // transform the data to {"x": x_value,"y": y_value} format and then push it to the temperature array created previously
        // see example below
        temp.forEach(row => {
        temperature.push({"x": row["_id"],"y": row["count"]})
        });
        // 5. Iterate through the humid variable, which contains humidity data fetched from the backend
        // transform the data to {"x": x_value,"y": y_value} format and then push it to the humidity array created previously
        // 6. Iterate through the humid variable, which contains heat index data fetched from the backend
        // transform the data to {"x": x_value,"y": y_value} format and then push it to the heatindex array created previously
        // 7. update series[0] for the histogram/Column chart with temperature data
        // see example below
        histogramChart.value.series[0].setData(temperature);
        // 8. update series[1] for the histogram/Column chart with humidity data
        // 9. update series[2] for the histogram/Column chart with heat index data
    }
}
</script>

<style scoped>
/** CSS STYLE HERE */
.row{
    max-width: 1200px;
}

.row1{
    padding: 1;
}

.sheet{
    padding: 2;
    height: 250;
}

.textField{
    max-width: 300px;
}

.textField1{
    margin-right: 5;
}
</style>
  