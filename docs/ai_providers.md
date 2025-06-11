# AI Provider Plugins

<<<<<< codex/implement-dynamic-ai-provider-loader
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
=======
`AIProvider` implementations supply text generation services for the
orchestrator.  Available providers are discovered from `providers.json` at the
repository root:

```json
{
  "openai": {"module": "openai_provider", "class": "OpenAIProvider"},
  "local-llama": {"module": "mock_provider", "class": "MockProvider"}
}
```

<<<<<< codex/implement-plugin-loader-hot-reload
Plugins can be hot-swapped at runtime in tests by reloading the provider loader.
The loader monitors ``providers.json`` and re-reads it whenever the file
modification time changes.  New providers become available to running
processes automatically on the next call to ``get_provider()``.  Errors while
loading are ignored so malformed entries do not interrupt execution.
=======
`scripts.ai_backend` and other helpers load this file on demand.  Plugins can be
hot-swapped in tests by reloading the loader module.

## Creating a custom AIProvider

1. Create a module under `scripts/ai_providers/`:

   ```python
   # scripts/ai_providers/my_provider.py
   from .base import AIProvider

   class MyProvider(AIProvider):
       """Return a canned reply for demonstration."""

       def generate(self, prompt: str) -> str:
           return "hello from my provider"
   ```

2. Register it in `providers.json`:

   ```json
   {
     "my-provider": {"module": "my_provider", "class": "MyProvider"}
   }
   ```

3. Retrieve the provider from the loader:

   ```python
   from scripts.ai_backend import _get_provider as get_provider

   prov = get_provider("my-provider")
   ```

   The active provider used by command line tools can also be overridden via the
   `AOS_AI_PROVIDER` environment variable:

   ```bash
   AOS_AI_PROVIDER=my-provider ./scripts/ai_backend.py "Hello"
   ```

See `scripts.ai_backend._get_provider()` for the loader implementation.
>>>>> main
>>>>> main
