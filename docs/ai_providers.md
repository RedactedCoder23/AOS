# AI Provider Plugins

Providers implement `AIProvider.generate(prompt) -> str` and are loaded from
`providers.json`. Example configuration:

```json
{
  "echo": "scripts.ai_providers.echo_provider.EchoProvider",
  "openai": "scripts.ai_providers.openai_provider.OpenAIProvider"
}
```

`echo` simply returns its prompt. The `openai` plugin forwards prompts to the
OpenAI ChatCompletion API. Plugins can be hot-swapped at runtime in tests by
reloading the provider loader.
