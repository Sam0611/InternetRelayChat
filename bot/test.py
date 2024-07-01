import requests

def get_translation_json(text, target_lang):
    auth_key = '59933f40-dc29-4c7c-b9a6-bce777bf4963:fx'
    url = "https://api-free.deepl.com/v2/translate"
    params = {
        "auth_key": auth_key,
        "text": text,
        "target_lang": target_lang
    }
    response = requests.post(url, data=params)
    return response.json()

if __name__ == "__main__":
    text = "Hello, how are you?"
    target_lang = "FR"
    json_response = get_translation_json(text, target_lang)
    print(json_response)
