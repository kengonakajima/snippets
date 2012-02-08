use {{ db.name }};

{% for t in db.tables %}
  {% if drop %}
drop table if exists {{ t.name }} ;
  {% endif %}
  {% if create %}
create table {{ t.name }} (
	{% for f in t.fields %}
    {{ f.name }} {{ f.type_in_sql }}{% if f.size %}({{f.size}}){% endif %}{% if f.primary %} primary key not null{% endif %}{% if f.auto_increment %} auto_increment{% endif %}{% if f.index %}, index({{f.name}}){% endif %} {% if not forloop.last %},{% endif %}
	{% endfor %}	
);
  {% endif %}
{% endfor %}


