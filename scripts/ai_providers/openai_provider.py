from scripts.ai_providers.base import AIProvider
import openai


class OpenAIProvider(AIProvider):
    """Real OpenAI provider using the ChatCompletion API."""

    def generate(self, prompt: str) -> str:
        resp = openai.ChatCompletion.create(
            model="gpt-3.5-turbo",
            messages=[{"role": "user", "content": prompt}],
        )
        return resp.choices[0].message["content"]
