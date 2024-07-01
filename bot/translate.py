import sys
import requests # allows to send HTTP requests in Python.

def translate_text(text, target_lang):
    auth_key = '59933f40-dc29-4c7c-b9a6-bce777bf4963:fx'
    url = "https://api-free.deepl.com/v2/translate"
    params = { # Dictionary containing request parameters.
        "auth_key": auth_key,
        "text": text,
        "target_lang": target_lang
    }
    response = requests.post(url, data=params)
    result = response.json()
    return result["translations"][0]["text"]
