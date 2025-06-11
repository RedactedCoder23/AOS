# AI Provider Plugins

Providers implement `AIProvider.generate(prompt) -> str` and are loaded from
`providers.json`. Example configuration:

```json
{
  "openai": {"module": "openai_provider", "class": "OpenAIProvider"},
  "echo": {"module": "mock_provider", "class": "MockProvider"}
}
```

Plugins can be hot-swapped at runtime in tests by reloading the provider loader.
