#ifdef __cplusplus
extern "C" {
#endif

struct shutdown_callback_data
{
	const char *deprecation;
};

void shutdown_callback(obs_data_t *request_data, obs_data_t *response_data, void *priv_data);

#ifdef __cplusplus
} // extern "C"
#endif
