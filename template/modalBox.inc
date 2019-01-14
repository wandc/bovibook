<?php
//uses bootstrap
class ModalBox {

    public $uniqueid;

    public function __construct($uniqueid) {

        $this->uniqueid = $uniqueid;
    }

    public function button() {
        $html = <<<HTML
        <!-- view modal -->
<button class="btn  btn-modal"
        data-toggle="modal"
        data-target="#fsModal-{$this->uniqueid}">
 <i class="ion ion-stats-bars"></i>
</button>
<!-- view modal -->
HTML;
        return $html;
    }

    public function popup($title,$content) {
        $html = <<<HTML
<!-- modal -->
<div id="fsModal-{$this->uniqueid}"
     class="modal animated bounceIn"
     tabindex="-1"
     role="dialog"
     aria-labelledby="myModalLabel"
     aria-hidden="true">

  <!-- dialog -->
  <div class="modal-dialog modal-xl">

    <!-- content -->
    <div class="modal-content">

      <!-- header -->
      <div class="modal-header">
        <h1 id="myModalLabel"
            class="modal-title">
        $title
        </h1>
      </div>
      <!-- header -->
      
      <!-- body -->
      <div class="modal-body">
       $content
      </div>
      <!-- body -->

      <!-- footer -->
      <div class="modal-footer">
        <button class="btn btn-secondary"
                data-dismiss="modal">
          close
        </button>
       
      </div>
      <!-- footer -->

    </div>
    <!-- content -->

  </div>
  <!-- dialog -->

</div>
<!-- modal -->

HTML;

        return $html;
    }

}//end classF