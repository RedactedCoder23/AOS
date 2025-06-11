import React from 'react';
import api from './services/api';

export default function RoleGuard({
  role,
  children,
}: {
  role: string;
  children: React.ReactElement;
}) {
  const current = api.getRole() || sessionStorage.getItem('role');
  if (current !== role) {
    return <div>forbidden</div>;
  }
  return children;
}
