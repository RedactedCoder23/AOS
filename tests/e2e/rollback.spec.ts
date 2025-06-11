import { test, expect } from "@playwright/test";

test("rollback button triggers confirm", async ({ page }) => {
  await page.route("**/branches/1/rollback", (route) => {
    route.fulfill({ status: 200, body: '{"rolled_back":true}' });
  });
  await page.goto("http://localhost:8000/branches/1");
  const dialogPromise = page.waitForEvent("dialog");
  await page.click("text=Rollback");
  const dialog = await dialogPromise;
  await dialog.accept();
  await expect(page.locator("text=Status")).toBeVisible();
});
