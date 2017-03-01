SELECT n.id_node, n.description, nld.id_local, nld.description, 
date_sub(m.time,interval second(m.time) second), 
((avg((io.a_conversion_factor*m.value + io.b_conversion_factor)*m.tension*nld.factor_potencia))*60*0.000000277778) 
FROM tesis.measure m INNER JOIN tesis.node_local_device nld ON m.id_node_device = nld.id_node_device 
INNER JOIN tesis.io_type io ON io.id_io = nld.id_io 
INNER JOIN tesis.node n ON n.id_node = nld.id_node
WHERE m.time BETWEEN date_sub(now(), interval 10 minute) 
AND now() GROUP BY minute(time), sensor;

INSERT INTO analytics.energy (id_node, node_description, id_local, local_description, time, energy_value)
SELECT (n.id_node, n.description, nld.id_local, nld.description, 
date_sub(m.time,interval second(m.time) second), 
((avg((io.a_conversion_factor*m.value + io.b_conversion_factor)*m.tension*nld.factor_potencia))*60*0.000000277778) 
FROM tesis.measure m INNER JOIN tesis.node_local_device nld ON m.id_node_device = nld.id_node_device 
INNER JOIN tesis.io_type io ON io.id_io = nld.id_io 
INNER JOIN tesis.node n ON n.id_node = nld.id_node
WHERE m.time BETWEEN date_sub(now(), interval 10 minute) 
AND now() GROUP BY minute(time), id_node, id_local);



#CORRE CADA 10 MINUTOS
CREATE PROCEDURE analytics
AS
BEGIN

INSERT INTO analytics.energy (id_node, node_description, id_local, local_description, time, energy_value)
SELECT (n.id_node, n.description, nld.id_local, nld.description, 
date_sub(m.time,interval second(m.time) second), 
((avg((io.a_conversion_factor*m.value + io.b_conversion_factor)*m.tension*nld.factor_potencia))*60*0.000000277778) 
FROM tesis.measure m INNER JOIN tesis.node_local_device nld ON m.id_node_device = nld.id_node_device 
INNER JOIN tesis.io_type io ON io.id_io = nld.id_io 
INNER JOIN tesis.node n ON n.id_node = nld.id_node
WHERE m.time BETWEEN date_sub(now(), interval 10 minute) 
AND now() GROUP BY minute(time), id_node, id_local); 



END;


CALL analytics;


DAILY energy
SELECT energy_value FROM analytics.energy WHERE id_node = ????? AND id_local = ????? AND time > date(now());

MONTHLY energy
SELECT sum(energy_value) FROM analytics.energy WHERE id_node = ????? AND id_local = ????? AND year(time) = year(now()) AND month(time) = month(now()) GROUP BY day(time);

COMPARATIVE ENRGY
SELECT GROUP BY id_node, id_local;


select MONTH(data), DAY(data), count(*), sum(case when status = 9 then valor else 0 end) as Aprroved,
 sum(case when status = 7 then valor else 0 end) as Pending, 
 sum(valor) as Total, 
 sum(case when status = 9 then valor else 0 end)*100/sum(valor) as CR 
 from apcd_documentos where merchant = 1736 and data > '2017-01-01' 
 group by MONTH(data), DAY(data);


AGREGAR TENSION A CODIGO insert de measure
agregar factor factor_potencia a insert codigo

