import { render, screen } from "@testing-library/react";
import "@testing-library/jest-dom";
import { BrowserRouter, Routes, Route } from "react-router-dom";
import RoleGuard from "../RoleGuard";
import AdminPage from "../pages/AdminPage";

test("blocks viewer", () => {
  sessionStorage.setItem("role", "viewer");
  render(
    <BrowserRouter>
      <Routes>
        <Route
          path="/"
          element={
            <RoleGuard role="admin">
              <AdminPage />
            </RoleGuard>
          }
        />
      </Routes>
    </BrowserRouter>
  );
  expect(screen.getByText(/forbidden/i)).toBeInTheDocument();
});

test("allows admin", () => {
  sessionStorage.setItem("role", "admin");
  render(
    <BrowserRouter>
      <Routes>
        <Route
          path="/"
          element={
            <RoleGuard role="admin">
              <AdminPage />
            </RoleGuard>
          }
        />
      </Routes>
    </BrowserRouter>
  );
  expect(screen.getByText(/Admin/i)).toBeInTheDocument();
});
