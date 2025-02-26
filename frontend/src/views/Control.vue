<template>
    <VCol>
        <VRow justify = "center">
            <v-sheets align = "center">
                <v-card justify = "center" title = "Combination" subtitle = "Enter your four-digit passcode" class = "text-secondary" color = "surface" variant = "tonal" flat>
                    <v-otp-input variant="solo" length = "4" error justify = "center"></v-otp-input>
                </v-card>
            
                <VBtn @click = "submitPW()" variant = "tonal" class = "btn">SUBMIT</VBtn>
            </v-sheets>
        </VRow>
    </VCol>
</template>

<script setup>
/** JAVASCRIPT HERE */

// IMPORTS
import { ref,reactive,watch ,onMounted,onBeforeUnmount,computed } from "vue";  
import { useRoute ,useRouter } from "vue-router";
import { useMqttStore } from '@/store/mqttStore'; // Import Mqtt Store
import { useAppStore } from "@/store/appStore"; 
import {storeToRefs} from "pinia";
 
 
// VARIABLES
const Mqtt = useMqttStore();
const AppStore = useAppStore();
const { payload, payloadTopic } = storeToRefs(Mqtt);  
const router = useRouter();
const route = useRoute();  
var pass = ref("0000");

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

const submitPW = async ()=>{
    await AppStore.setPW(pass.value);
};
</script>


<style scoped>
/** CSS STYLE HERE */
.btn{
    background-color:olive;
}

</style>
  