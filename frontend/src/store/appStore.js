import {defineStore} from 'pinia'
import {ref} from 'vue'


export const useAppStore =  defineStore('app', ()=>{

    /*  
    The composition API way of defining a Pinia store
    ref() s become state properties
    computed() s become getters
    function() s become actions  
    */ 
   
    const setPW = async (passcode) => {
        const toast_status = {
          title: "Passcode Update Request",
          body: "Your passcode is attempting to update",
          mode: "loader",
        };
    }
 
 
    return { 
    // EXPORTS	
      setPW,
        
    };
},{ persist: true  });