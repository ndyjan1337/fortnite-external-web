function toggleStatus(element) {
    var status = element.querySelector('.status');
    if (status.classList.contains('on')) {
        status.classList.remove('on');
        status.classList.add('off');
        status.textContent = 'OFF';
    } else {
        status.classList.remove('off');
        status.classList.add('on');
        status.textContent = 'ON';
    }
}

function updateSliderValue(slider) {
    var valueDisplay = document.getElementById('slider-value');
    valueDisplay.textContent = slider.value;
}
