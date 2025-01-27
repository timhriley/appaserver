:
table=water_project_unit
field="project_name,parameter,units"

echo "select $field from $table;"				|
sql								|
insert_statement table=$table field=$field del='^' compress=y	|
cat
