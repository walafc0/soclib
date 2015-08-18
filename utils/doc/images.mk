

IMG_SRC:=$(shell find images/ -name \*.fig)

build_images=$(patsubst images/%.fig,_static/%.$(1),$(IMG_SRC))

IMG_png_OPTS:=-S 4 -Z 10cm

define do_img

_static/$(1).%: images/$(1).fig _static
	fig2dev -L $$* $$(IMG_$$(*)_OPTS) $$< > $$@ || rm $$@

endef

_static:
	mkdir -p $@

$(eval $(foreach v,$(patsubst images/%.fig,%,$(IMG_SRC)),$(call do_img,$(v))))

define image_clean
	-rm -f $$(call build_images,*)
endef
