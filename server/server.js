// server.mjs

import express from "express";
import cors from "cors";
import fetch from "node-fetch";
import dotenv from "dotenv";
import { OpenAI } from "openai";

dotenv.config();

const app = express();
app.use(cors());
app.use(express.json());

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

const ESP32_URL = "http://192.168.3.118"; // Your ESP32 IP address in AP mode

app.post("/chat", async (req, res) => {
  const userMessage = req.body.message;
  console.log(`chat called`);

  const prompt = `
You are a command interpreter for an LED controller. 
Given the user's message, respond with only one of: "on", "off", or "none". Please understand intent such as "make it brighter in here".
User: "${userMessage}"
`;
  console.log(`prompt: ` + prompt);

  try {
    const completion = await openai.chat.completions.create({
      model: "gpt-4",
      messages: [{ role: "user", content: prompt }],
      max_tokens: 5,
    });

    const intent = completion.choices[0].message.content.trim().toLowerCase();
    console.log(`Parsed intent: "${intent}"`);

    if (intent === "on" || intent === "off") {
      const espResponse = await fetch(`${ESP32_URL}/led/${intent}`);
      const text = await espResponse.text();
      return res.json({ status: text });
    } else {
      return res.json({ status: "Sorry, I didn't understand that command." });
    }
  } catch (err) {
    console.log("ERROR");
    console.error(err);
    return res
      .status(500)
      .json({ status: "Error talking to ChatGPT or ESP32." });
  }
});

app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
