# merge_ai Helper

The `merge_ai.py` script performs an LLM-assisted three-way merge.

## Hunk-based algorithm

1. A combined unified diff is generated with
   `git diff --no-prefix --unified=0 BASE MAIN BRANCH`.
2. The diff is segmented into hunks at each `diff --git` header or `@@` marker.
   Each hunk is at most `MAX_HUNK_SIZE` bytes (4096 by default).
3. Every hunk is sent to the LLM with the prompt:

   ```
   Resolve this diff hunk between MAIN, BASE, and BRANCH. Return a valid patch snippet.
   ```
4. If the response does not apply cleanly, the original hunk is wrapped with
   conflict markers:

   ```
   <<<<< CONFLICT
   [original hunk]
   >>>>>
   ```
5. The resulting snippets are concatenated to form the final patch. The script
   logs the size and processing time of each hunk.


## Fallback Behavior
If the language model returns an empty response or a patch that fails `git apply --check`,
`merge_ai` wraps the original hunk in conflict markers. This ensures the final patch
always applies and clearly highlights regions that require manual intervention.
