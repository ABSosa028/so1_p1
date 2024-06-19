import './App.css';
import PieChart from './components/pie_chart/pie_chart.jsx';
import {useState, useEffect} from 'react';
import TablaProcesos from './components/tabla_procesos/TablaProcesos.jsx';

function App() {
	const [mem, setMem] = useState(null);
	const [cpu, setCpu] = useState(null);
	const [data, setData] = useState(null);
	const [optsSim, setOptsSim] = useState([]);
	const [simActual, setSimActual] = useState(-1);
	
	

	useEffect(()=>{
      const interval = setInterval(()=> get_datos(), 1000);
      return () => clearInterval(interval);
    },[]);

  useEffect(()=>{
	const interval = setInterval(()=> get_procesos(), 1000);
	return () => clearInterval(interval);
  },[]);
  
  
	function get_datos() {
		fetch('/data')
			.then((res) => res.json())
	    .then((d) => {
			console.log(d)
	    	d = JSON.parse(d);
	    	
		  	const nMem = {      
		  		labels:["En uso", "Libre"],
		  		label: "%",
		  		data: [d.memoria_ocupada, 100-d.memoria_ocupada],
		  		color: ["#6488ea","#282c34"],
		  		title: "Memoria Ram"
    		}
    		
    		setMem(nMem);
    		
    		let utilizacion = 100*d.cpu_utilizado/2;
    		utilizacion = Math.round(utilizacion*100)/100;
    		let liberada = Math.round((100-utilizacion)*100)/100;
    		
    		if(utilizacion > 100){
    			utilizacion = 100;
    			liberada = 0;
    		}
    		
    		const nCpu = {      
		  		labels:["En uso", "Libre"],
		  		label: "%",
		  		data: [utilizacion, liberada],
		  		color: ["#6488ea","#282c34"],
		  		title: "CPU"
    		}
    		setCpu(nCpu);

  	}).catch(error => {console.log(error);});
	};
	
	function get_procesos() {
		fetch('/data')
				.then((res) => res.json())
			.then((d) => {
				d = JSON.parse(d);
				setData(d.procesos.filter(p=>p.nombre == "sleep"));
	}).catch(error => {console.log(error);});
	};
	
	function cambioSelSim(e){
		setSimActual(e.target.value);
	}
	
	const generaNuevo = ()=>{
		
		fetch('/iniciarP')
		.catch(error => {console.log(error);});
	};
	
	const terminarSim = ()=>{
		console.log(simActual)
		if(simActual == -1){
			alert("no se ha seleccionado un proceso");
			return
		}
		fetch('/terminarP', {
		  method: 'POST',
		  headers: {
		    'Accept': 'application/json',
		    'Content-Type': 'application/json'
		  },
		  body: JSON.stringify({
				"pid": parseInt(simActual)
			})
		});
		alert("proceso terminado.");
	}
	
	return (
	<div className="App">
		<section className="sectionTR">
			<h1>Monitoreo en Tiempo Real</h1>
			<div className="gPie">
				<div className="gContainer">
			  		{mem ? <PieChart d={mem}/> : <h1>cargando...</h1>}
			  	</div>
			  	<div className="gContainer">
			  		{cpu ? <PieChart d={cpu}/> : <h1>cargando...</h1>}
			  	</div>
			</div>
	  </section>
	  <section className="sectionAP">
		<h1>Tabla de Procesos</h1>
		{data && <TablaProcesos procesos={data} />}
		<div>
			<input onChange={e => cambioSelSim(e)}>

			</input>
			<button onClick={generaNuevo}>Nuevo</button>
			<button onClick={terminarSim}>Terminar</button>
		</div>
	  </section>
	  
	</div>
	);
}

export default App;
