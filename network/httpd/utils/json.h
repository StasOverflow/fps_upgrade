

#ifndef JSON_H_
#define JSON_H_


char* json_begin(char *p);
char* json_node_add(char *p, const char *name, const char *value);
char*  json_end(char *p);


#endif /* JSON_H_ */
