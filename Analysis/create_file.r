startup<-read.table("/home/wyatt/Documents/Arboreal/Source/Data/create_file_time.txt", header = TRUE)


hist((startup$numtags))
startup<-startup[startup$numtags > 0,]
startup<-startup[startup$time < 0.2,]

plot(time~files, data = startup, main ="CreateFile: Time ~ Files")
plot(time~tags, data = startup, main ="CreateFile: Time ~ Tags")
plot(log(time)~numtags, data = startup)


fit<-lm(time~numtags + I(numtags^2), data = startup)
summary(fit)
plot(fit)

plot(fit$residuals~startup$numtags,ylab="Residuals", xlab="NumTags", main="NumTags Residual Plot")


plot(time~numtags, data = startup, main="CreateFile: Time ~ NumTags")

round(fit$coefficients, 6)
c<-fit$coefficients
cbind(c, seq(3))
summary(fit)

numtagsHat<-seq(0,20, length=1000)
timeHat<-c[1] + c[2]*numtagsHat + c[3]*numtagshat^2
lines(timeHat~numtagsHat, col="red", lwd = 3)

legend("bottomleft", c("Temp=-120", "Temp=50", "Temp=195"), col=c("red", "blue", "green"), lty=1, lwd = 3)
