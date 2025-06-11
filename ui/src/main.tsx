import React from "react";
import ReactDOM from "react-dom/client";
import { BrowserRouter, Routes, Route } from "react-router-dom";
import BranchList from "./pages/BranchList";
import BranchDetail from "./pages/BranchDetail";

ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<BranchList />} />
        <Route path="/branches/:id" element={<BranchDetail />} />
      </Routes>
    </BrowserRouter>
  </React.StrictMode>,
);
