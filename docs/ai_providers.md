# AI Provider Plugins

Providers implement `AIProvider.generate(prompt) -> str` and are loaded from
`providers.json`.

## Plugin loader

The file maps a provider alias to a fully qualified class path. Example
`providers.json`:

```json
{
  "openai": "scripts.ai_providers.openai_provider.OpenAIProvider",
  "echo": "scripts.ai_providers.echo_provider.EchoProvider"
}
```

Call `get_provider(alias)` to retrieve an instance:

```python
from scripts.ai_providers.loader import get_provider
provider = get_provider("openai")
```

Plugins can be hot-swapped at runtime in tests by reloading the loader module.
