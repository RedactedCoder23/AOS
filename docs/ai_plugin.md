# AI Provider Plugin Architecture

This system allows dynamic discovery of AI providers used for language tasks.

## AIProvider Interface

```python
class AIProvider:
    def __init__(self, name: str):
        self.name = name

    def generate(self, prompt: str) -> str:
        """Return text generated for *prompt*."""
```

## Adding a Provider

1. Implement the `AIProvider` interface in `scripts/ai_providers/<module>.py`.
2. Add an entry to `providers.json` at the repository root:

```json
{
  "my-provider": { "module": "my_module", "class": "MyProvider" }
}
```

3. On startup, the orchestrator and AI backend load this file, import the
   module, and instantiate the class.

## Example providers.json

```json
{
  "openai": { "module": "openai_provider", "class": "OpenAIProvider" },
  "local-llama": { "module": "llama_provider", "class": "LlamaProvider" }
}
```

## Task Configuration

Tasks may select a provider via the `provider` field in `tasks.yaml`:

```yaml
- agent_id: "doc-gen"
  command: "generate docs"
  provider: "local-llama"
```

During execution the orchestrator chooses the matching provider instance for the
task.
