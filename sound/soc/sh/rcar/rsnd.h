/*
 * Renesas R-Car
 *
 * Copyright (C) 2013 Renesas Solutions Corp.
 * Kuninori Morimoto <kuninori.morimoto.gx@renesas.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef RSND_H
#define RSND_H

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/module.h>
#include <sound/rcar_snd.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>

/*
 *	pseudo register
 *
 * The register address offsets SRU/SCU/SSIU on Gen1/Gen2 are very different.
 * This driver uses pseudo register in order to hide it.
 * see gen1/gen2 for detail
 */
struct rsnd_priv;
struct rsnd_dai;
struct rsnd_dai_stream;

/*
 *	R-Car sound DAI
 */
#define RSND_DAI_NAME_SIZE	16
struct rsnd_dai_stream {
	struct list_head head; /* head of rsnd_mod list */
	struct snd_pcm_substream *substream;
	int byte_pos;
	int period_pos;
	int byte_per_period;
	int next_period_byte;
};

struct rsnd_dai {
	char name[RSND_DAI_NAME_SIZE];
	struct rsnd_dai_platform_info *info; /* rcar_snd.h */
	struct rsnd_dai_stream playback;
	struct rsnd_dai_stream capture;

	int clk_master:1;
	int bit_clk_inv:1;
	int frm_clk_inv:1;
	int sys_delay:1;
	int data_alignment:1;
};

#define rsnd_dai_nr(priv) ((priv)->dai_nr)
#define for_each_rsnd_dai(rdai, priv, i)		\
	for (i = 0, (rdai) = rsnd_dai_get(priv, i);	\
	     i < rsnd_dai_nr(priv);			\
	     i++, (rdai) = rsnd_dai_get(priv, i))

struct rsnd_dai *rsnd_dai_get(struct rsnd_priv *priv, int id);
int rsnd_dai_is_play(struct rsnd_dai *rdai, struct rsnd_dai_stream *io);
#define rsnd_dai_get_platform_info(rdai) ((rdai)->info)

void rsnd_dai_pointer_update(struct rsnd_dai_stream *io, int cnt);
int rsnd_dai_pointer_offset(struct rsnd_dai_stream *io, int additional);

/*
 *	R-Car sound priv
 */
struct rsnd_priv {

	struct device *dev;
	struct rcar_snd_info *info;
	spinlock_t lock;

	/*
	 * below value will be filled on rsnd_dai_probe()
	 */
	struct snd_soc_dai_driver *daidrv;
	struct rsnd_dai *rdai;
	int dai_nr;
};

#define rsnd_priv_to_dev(priv)	((priv)->dev)
#define rsnd_lock(priv, flags) spin_lock_irqsave(&priv->lock, flags)
#define rsnd_unlock(priv, flags) spin_unlock_irqrestore(&priv->lock, flags)

#endif