# AI Provider Plugins

`AIProvider` implementations supply text generation services for the orchestrator. Providers are discovered from `providers.json` at the repository root.

Example `providers.json`:

```json
{
  "openai": { "module": "openai_provider", "class": "OpenAIProvider" },
  "local-llama": { "module": "mock_provider", "class": "MockProvider" },
  "echo": "scripts.ai_providers.echo_provider.EchoProvider"
}
```

Use `get_provider(alias)` to retrieve an instance:

```python
from scripts.ai_providers.loader import get_provider
provider = get_provider("openai")
```

The loader monitors the configuration file and reloads plugins when it changes so tests can hot-swap providers.

## Creating a custom AIProvider

1. Create a module under `scripts/ai_providers/`:

   ```python
   # scripts/ai_providers/my_provider.py
   from .base import AIProvider

   class MyProvider(AIProvider):
       def generate(self, prompt: str) -> str:
           return "hello from my provider"
   ```

2. Register it in `providers.json`:

   ```json
   {
     "my-provider": { "module": "my_provider", "class": "MyProvider" }
   }
   ```

3. Retrieve the provider from the loader:

   ```python
   from scripts.ai_providers.loader import get_provider
   prov = get_provider("my-provider")
   ```

The active provider used by command line tools can also be overridden via the `AOS_AI_PROVIDER` environment variable:

```bash
AOS_AI_PROVIDER=my-provider ./scripts/ai_backend.py "Hello"
```

> **Warning**
> Provider classes must override `generate`. The loader raises
> `ProviderImplementationError` if this method is missing.
