/*


	size_t pool_size = 500;
	size_t chunk_size = 50;
	PoolMemory *pool = make_pool(pool_size, chunk_size);

	int poolSpace = calculate_space(sizeof(PoolMemory), sizeof(size_t));
	int j = 0;
	void *pools[10];

	while (game_loop())
	{

		size_t start = (size_t)pool - pool->padding;
		PoolMemoryNode *node = (PoolMemoryNode *)pool->head;
		int i = 0;
		float last = (float)pool_size;
		while (node != NULL)
		{

			size_t offset;
			char used;
			byte7d(node->data, &offset, &used);

			float cur = (float)(offset);
			Vec3 al = Vec3{0, last, 10};
			Vec3 an = Vec3{0, cur, 10};
			Vec3 be = Vec3{0, cur + chunk_size, 10};
			Color c = used ? color_yellow : color_red;
			draw_line(an, be, color_red);
			draw_line(al, be, color_gray);

			last = cur;
			node = (PoolMemoryNode *)(start + offset);
			i++;

			if (offset == pool->padding + poolSpace)
				node = NULL;
		}

		if (input_keydown(KEY_SPACE))
		{
			if (j < 10)
			{
				void *ptr = pool_alloc(pool);
				if (ptr != NULL)
				{
					pools[j++] = ptr;
				}
			}
		}

		if (input_keydown(KEY_M))
		{
			if (j > 0)
				pool_free(pool, &pools[--j]);
		}
	}


*/