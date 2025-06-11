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
The loader monitors ``providers.json`` and re-reads it whenever the file
modification time changes.  New providers become available to running
processes automatically on the next call to ``get_provider()``.  Errors while
loading are ignored so malformed entries do not interrupt execution.
