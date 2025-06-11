import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import Marketplace from "../pages/Marketplace";

test("renders marketplace header", () => {
  render(<Marketplace />);
  expect(screen.getByText(/Marketplace/i)).toBeInTheDocument();
});
