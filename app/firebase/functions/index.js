/**
 * Import function triggers from their respective submodules:
 *
 * const {onCall} = require("firebase-functions/v2/https");
 * const {onDocumentWritten} = require("firebase-functions/v2/firestore");
 *
 * See a full list of supported triggers at https://firebase.google.com/docs/functions
 */

const { onRequest } = require("firebase-functions/v2/https");
const logger = require("firebase-functions/logger");
const { initializeApp } = require("firebase-admin/app");
const { getFirestore } = require("firebase-admin/firestore");
// Create and deploy your first functions
// https://firebase.google.com/docs/functions/get-started

// export const FIRESTORE_EMULATOR_HOST = "127.0.0.1:8080"


initializeApp();
exports.helloWorld = onRequest((request, response) => {
  logger.info("Hello logs!", { structuredData: true });
  response.send("Hello from Firebase!");
});

const uuid = "37116885-c58c-4416-8662-471e917f6812"

exports.onCreate = onRequest(async (request, response) => {
  const dbReponse = await getFirestore().collection("heatpump").get(uuid);
  if (dbReponse.empty) {
    await getFirestore().collection("heatpump").doc(uuid).set({});
    response.json({ result: `document ${uuid} created` });
  } else {
    response.json({ result: `Collection already exists` });
  }
})
exports.getState = onRequest(async (request, response) => {
  const dbReponse = await getFirestore().collection("heatpump").doc(uuid).get();
  response.json({ result: JSON.stringify(dbReponse.data(), null, 2) });
})

exports.setState = onRequest((request, response) => {

})

// {
//    name:string,
//    unit_on_off_state: boolean,
//    heat_cool_state:boolean,
//    dhw_state:boolean,
//    heating_cooling_state:boolean,
// }