
  // Import the functions you need from the SDKs you need
  import { initializeApp } from "https://www.gstatic.com/firebasejs/9.17.2/firebase-app.js";
  
  import { getDatabase, ref, onValue } from "https://www.gstatic.com/firebasejs/9.17.2/firebase-database.js";
  // TODO: Add SDKs for Firebase products that you want to use
  // https://firebase.google.com/docs/web/setup#available-libraries

  // Your web app's Firebase configuration
  // For Firebase JS SDK v7.20.0 and later, measurementId is optional
  const firebaseConfig = {
    apiKey: "AIzaSyAqdzU2tHHXmFe9B8Ww7HHVwbfI2ftax2s",
    authDomain: "sensor-turbidez.firebaseapp.com",
    databaseURL: "https://sensor-turbidez-default-rtdb.firebaseio.com",
    projectId: "sensor-turbidez",
    storageBucket: "sensor-turbidez.appspot.com",
    messagingSenderId: "593130848746",
    appId: "1:593130848746:web:dfb97bfbb8fd0adf06b293",
    measurementId: "G-F1LQ6QTH1N"
  };

  // Initialize Firebase
const app = initializeApp(firebaseConfig);

const database = getDatabase();
const data = ref(database)

const turbidez = document.getElementById('turbidez')

onValue(data, (value) => {
    turbidez.innerText = value.val().agua.turbidez;
})