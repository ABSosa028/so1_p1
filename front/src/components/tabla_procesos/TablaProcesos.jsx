import React from 'react';
import './TablaProcesos.css';


const TablaProcesos = ({ procesos }) => {
  return (
    <div className="table-container">
      <table className="table">
        <thead>
          <tr>
            <th>PID</th>
            <th>Estado</th>
            <th>Nombre</th>
          </tr>
        </thead>
        <tbody>
          {procesos.map((proceso, index) => (
            <tr key={index}>
              <td>{proceso.pid}</td>
              <td>{proceso.estado}</td>
              <td>{proceso.nombre}</td>
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
};

export default TablaProcesos;
