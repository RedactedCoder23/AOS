import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import BranchList from "../pages/BranchList";

test("renders header", () => {
  render(<BranchList />);
  expect(screen.getByText(/Branches/i)).toBeInTheDocument();
});
