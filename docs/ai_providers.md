# AI Provider Plugins

`AIProvider` implementations supply text generation services for the
orchestrator.  Available providers are discovered from `providers.json` at the
repository root:

```json
{
  "openai": {"module": "openai_provider", "class": "OpenAIProvider"},
  "local-llama": {"module": "mock_provider", "class": "MockProvider"}
}
```

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
