
def routine_calculate_analytics(cursor):
	query = ("INSERT INTO analytics.energy (id_node, node_description, id_local, local_description, time, energy_value) SELECT n.id_node, n.description, nld.id_local, nld.description, date_sub(m.time,interval second(m.time) second), ((avg(((io.a_conversion_factor*m.value) + io.b_conversion_factor)*m.tension*nld.factor_potencia))*60*0.000000277778) FROM tesis.measure m INNER JOIN tesis.node_local_device nld ON m.id_node_device = nld.id_node_device INNER JOIN tesis.io_type io ON io.id_io = nld.id_io INNER JOIN tesis.node n ON n.id_node = nld.id_node WHERE m.time BETWEEN date_sub(now(), interval 10 minute) AND now() GROUP BY minute(time), id_node, id_local");
	cursor.execute(query)
